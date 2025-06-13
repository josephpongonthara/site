import pandas as pd
from tabulate import tabulate
import matplotlib.pyplot as plt

df = pd.read_csv("/Users/josephpongonthara/Desktop/FANTASY BALL/2023-2024 NBA Player Stats - Regular.csv",
                 delimiter=';', encoding='ISO-8859-1')

# Normalize player names to avoid encoding issues
df['Player'] = df['Player'].str.normalize('NFKD')\
                           .str.encode('ascii', errors='ignore')\
                           .str.decode('utf-8')

file_path = '/Users/josephpongonthara/Desktop/FANTASY BALL/23-24_Fantasy_League_Averages.xlsx'
league1 = pd.read_excel(file_path, sheet_name='EVL_14')
league2 = pd.read_excel(file_path, sheet_name='GLEAGUE_11')

def calculate_winning_scores(league):
    scores = {
        'FG%': [], 'FT%': [], '3PTM': [], 'PTS': [],
        'REB': [], 'AST': [], 'ST': [], 'BLK': [], 'TO': []
    }
    for _, row in league.iterrows():
        scores['FG%'].append(max(row['FG%'], row['OPP FG%']))
        scores['FT%'].append(max(row['FT%'], row['OPP FT%']))
        scores['3PTM'].append(max(row['3PTM'], row['OPP 3PTM']))
        scores['PTS'].append(max(row['PTS'], row['OPP PTS']))
        scores['REB'].append(max(row['REB'], row['OPP REB']))
        scores['AST'].append(max(row['AST'], row['OPP AST']))
        scores['ST'].append(max(row['ST'], row['OPP ST']))
        scores['BLK'].append(max(row['BLK'], row['OPP BLK']))
        scores['TO'].append(min(row['TO'], row['OPP TO']))  # Min for TO

    return pd.DataFrame(scores).mean()

average_winning_scores = calculate_winning_scores(league1)

# ========== TRACK DRAFTED PLAYERS ==========
drafted_players = []
remainders = {}

def update_team_with_player(player_name, df, avg_scores):
    global remainders
    stats = df[df['Player'] == player_name]
    if stats.empty:
        print(f"Player '{player_name}' not found.\n")
        return

    drafted_players.append(stats)

    team_totals = {
        '3PTM': 0, 'PTS': 0, 'REB': 0, 'AST': 0,
        'ST': 0, 'BLK': 0, 'TO': 0, 'FG%': 0, 'FT%': 0
    }

    for p in drafted_players:
        team_totals['3PTM'] += p['3P'].values[0] * 3
        team_totals['PTS']  += p['PTS'].values[0] * 3
        team_totals['REB']  += p['TRB'].values[0] * 3
        team_totals['AST']  += p['AST'].values[0] * 3
        team_totals['ST']   += p['STL'].values[0] * 3
        team_totals['BLK']  += p['BLK'].values[0] * 3
        team_totals['TO']   += p['TOV'].values[0] * 3
        team_totals['FG%']  += p['FG%'].values[0]
        team_totals['FT%']  += p['FT%'].values[0]

    num = len(drafted_players)
    fg_avg = team_totals['FG%'] / num if num else 0
    ft_avg = team_totals['FT%'] / num if num else 0

    remainders = {
        'FG%': avg_scores['FG%'] - fg_avg,
        'FT%': avg_scores['FT%'] - ft_avg,
        '3PTM': avg_scores['3PTM'] - team_totals['3PTM'],
        'PTS': avg_scores['PTS'] - team_totals['PTS'],
        'REB': avg_scores['REB'] - team_totals['REB'],
        'AST': avg_scores['AST'] - team_totals['AST'],
        'ST': avg_scores['ST'] - team_totals['ST'],
        'BLK': avg_scores['BLK'] - team_totals['BLK'],
        'TO': avg_scores['TO'] - team_totals['TO'],
    }

    print(f"\n Drafted: {player_name}")
    print("Updated remainders to meet winning averages:")
    for k, v in remainders.items():
        print(f"{k}: {v:.2f}")

# ========== RECOMMENDATION FUNCTION ==========
def recommend_players_to_draft(df, drafted_players_list, remainders, target_categories, max_recommendations=5):
    available = df[~df['Player'].isin(drafted_players_list)]
    recommendations = []

    for _, player in available.iterrows():
        score = 0
        deltas = {}
        for cat in target_categories:
            if cat == '3PTM':
                val = player['3P'] * 3
            elif cat == 'ST':
                val = player['STL'] * 3
            elif cat == 'BLK':
                val = player['BLK'] * 3
            elif cat == 'PTS':
                val = player['PTS'] * 3
            elif cat == 'AST':
                val = player['AST'] * 3
            elif cat == 'REB':
                val = player['TRB'] * 3
            elif cat in ['FG%', 'FT%', 'TO']:
                val = player[cat]
            else:
                continue
            delta = val - remainders[cat]
            deltas[cat] = delta
            score += delta
        recommendations.append((player['Player'], score, deltas))

    recommendations.sort(key=lambda x: x[1])
    return recommendations[:max_recommendations]

def print_recommendations(recommendations):
    if not recommendations:
        print("No recommendations available.")
        return

    headers = ["Player", "Score"] + list(recommendations[0][2].keys())
    table = []
    for name, score, deltas in recommendations:
        row = [name, f"{score:.2f}"] + [f"{v:.2f}" for v in deltas.values()]
        table.append(row)
    print("\n Recommended Players:")
    print(tabulate(table, headers=headers, tablefmt="fancy_grid"))

# ========== OPTIONAL: VISUAL REMAINDERS ==========
def plot_remainders(remainders_dict):
    plt.figure(figsize=(10, 5))
    plt.barh(list(remainders_dict.keys()), list(remainders_dict.values()), color='teal')
    plt.title("Remaining Category Gaps to Reach Winning Average")
    plt.axvline(0, color='black')
    plt.tight_layout()
    plt.show()

# ========== EXAMPLE USAGE ==========
if __name__ == "__main__":
    # Draft a few players
    player_names = [
        'Anthony Edwards', 'Scottie Barnes', 'Jalen Williams', 'Lauri Markkanen',
        'Nikola Vucevic', 'Jalen Duren', 'Coby White', 'Daniel Gafford',
        'Bogdan Bogdanovic', 'Jonathan Kuminga', 'Jordan Clarkson',
        'Onyeka Okongwu', 'Bennedict Mathurin'
    ]
    for p in player_names:
        update_team_with_player(p, df, average_winning_scores)

    # Get recommendations
    target_categories = ['3PTM', 'ST', 'BLK']
    drafted_names = [p['Player'].values[0] for p in drafted_players]
    recs = recommend_players_to_draft(df, drafted_names, remainders, target_categories)

    # Display
    print_recommendations(recs)
    plot_remainders(remainders)
